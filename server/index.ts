import express, { Request, Response } from 'express';

import "dotenv/config";

import { connectionToMongo } from './database/mongo';
import { TcpServer, broadcast, gamesSessions, sessions } from './tcp/tcp';
import expressAsyncHandler from 'express-async-handler';
import { Thresholds, generateRandIndex, nid } from './utils/helpers';
import { Session } from './tcp/session';

let thresholds = Thresholds;

const main = async () => {
    const mongo = await connectionToMongo();
    const PORT = process.env.PORT || 3000;
    const app = express();
    app.use(express.json());


    app.post('/deviceMap', expressAsyncHandler(async (req: Request, res: Response) => {
        const map = req.body;
        await mongo.collection('devices').updateOne({ _id: map.measStation }, { $set: { _id: map.measStation, devices: map.buttStations }}, { upsert: true });
        res.sendStatus(200);
    }))

    app.get('/thresholds', (req: Request, res: Response) => {
        res.status(200).json(thresholds);
    })

    app.post('/thresholds', (req: Request, res: Response) => {
        thresholds = req.body;
        console.log(thresholds);
        res.sendStatus(200);
    })

    app.post('/measurements', expressAsyncHandler(async (req: Request, res: Response) => {
        const body = req.body;
        const measurements = { date: new Date(), ...body.measurements };
        const devices = (await mongo.collection<Station>('devices').findOne({ _id: body.stationId }))?.devices;
        console.log(devices);
        let data: any = {};
        if (measurements.temperature > 25) {
            data.temperature = 'Temperature too high!' 
            // console.log('Temperatura za wysoka!');
        }
        if (measurements.temperature < 20) {
            data.temperature = 'Temperature too low!' 
            // console.log('Temperatura za niska!')
        }
        if (measurements.humidity < 40) {
            data.humidity = 'Humidity to low!'
            // console.log('Za niska wigotność!')
        }
        if (measurements.humidity > 60) {
            data.humidity = 'Humidity to high!'
            // console.log('Za wysoka wilgotność!')
        }
        if (measurements.noise) data.noise = measurements.noise
        
        if (Object.keys(data).length) {
            console.log(data);
            broadcast(JSON.stringify({ type: 'measurements', ...data }), (s: Session) => !s.getSended && !!devices?.includes(s.getId));
        }
        res.sendStatus(200);
    }))

    app.post('/game', (req: Request, res: Response) => {
        const game = req.body;
        mongo.collection('games').insertOne(game);
        res.sendStatus(200);
    });

    app.post('/takeBreak/:maintainerId', expressAsyncHandler(async (req: Request, res: Response) => {
        const randGames: Game[] = await mongo.collection<Game>('games').find().toArray();
        const randGame = randGames[generateRandIndex(randGames.length)];
        const game = {
            id: nid(),
            maintainer: req.params.maintainerId,
            players: [],
            game: randGame,
        };
        gamesSessions.push(game);
        console.log(game)
        broadcast(JSON.stringify({
            type: 'game',
            gameId: game.id,
            name: game.game.name,
            members: game.game.members.toString(),
            players: '1'
        }), (s: Session) => !s.getBusy, true);

    }));
    // const watcher = mongo.collection('measurements').watch()

    const server = app.listen(PORT, () => console.log(`Server running on port ${PORT}`));


    const tcpServer = TcpServer.getInstance();
    tcpServer.init(
        "0.0.0.0",
        9000,
    );


};

main();