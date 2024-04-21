import express, { Request, Response } from 'express';

import "dotenv/config";

import { connectionToMongo } from './database/mongo';
import { TcpServer, broadcast, gamesSessions, getSession, sessions } from './tcp/tcp';
import expressAsyncHandler from 'express-async-handler';
import { Thresholds, generateRandIndex, nid } from './utils/helpers';
import { Session } from './tcp/session';

export let thresholds: any;
export let noOfActivitiesPerDay = 60000;
export let delayBetweenActivities = 60000;

const main = async () => {
    const mongo = await connectionToMongo();
    const PORT = process.env.PORT || 3000;
    const app = express();
    app.use(express.json());


    thresholds = await mongo.collection('config').findOne({ _id: 'thresholds' as any }) || Thresholds;
    app.post('/timers', (req: Request, res: Response) => {
        noOfActivitiesPerDay = req.body.noOfActivitiesPerDay;
        delayBetweenActivities = req.body.delayBetweenActivities;
        res.sendStatus(200);
    })


    app.post('/deviceMap', expressAsyncHandler(async (req: Request, res: Response) => {
        const map = req.body;
        await mongo.collection('devices').updateOne({ _id: map.measStation }, { $set: { _id: map.measStation, devices: map.buttStations } }, { upsert: true });
        res.sendStatus(200);
    }))

    app.get('/thresholds', (req: Request, res: Response) => {
        res.status(200).json(thresholds);
    })

    app.post('/thresholds', expressAsyncHandler(async (req: Request, res: Response) => {
        thresholds = req.body;
        await mongo.collection('config').updateOne({ _id: 'thresholds' as any }, { $set: req.body }, { upsert: true });
        res.sendStatus(200);
    }))

    app.post('/measurements', expressAsyncHandler(async (req: Request, res: Response) => {
        const body = req.body;
        const measurements = { date: new Date(), ...body.measurements };
        const devices = (await mongo.collection<Station>('devices').findOne({ _id: body.stationId }))?.devices;
        console.log(devices);
        let data: any = {};
        if (measurements.temperature > thresholds.temperatures.max) {
            data.temperature = 'Temperature too high!'
        }
        if (measurements.temperature < thresholds.temperatures.min) {
            data.temperature = 'Temperature too low!'
        }
        if (measurements.humidity < thresholds.humidity.min) {
            data.humidity = 'Humidity to low!'
        }
        if (measurements.humidity > thresholds.humidity.max) {
            data.humidity = 'Humidity to high!'
        }
        if (measurements.noise) data.noise = measurements.noise.toString()

        if (Object.keys(data).length) {
            console.log(data);
            broadcast(JSON.stringify({ type: 'measurements', ...data }), (s: Session) => !s.getSended && !!devices?.includes(s.getId));
        } else {
            broadcast(JSON.stringify({ type: 'measurements' }), (s: Session) => !s.getSended && !!devices?.includes(s.getId));
        }
        res.sendStatus(200);
    }))

    app.post('/game', (req: Request, res: Response) => {
        const game = req.body;
        mongo.collection('games').insertOne(game);
        res.sendStatus(200);
    });

    app.post('/takeGame/:maintainerId', expressAsyncHandler(async (req: Request, res: Response) => {
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
        }), (s: Session) => !s.getBusy && !s.getTimer, true);

    }));

    app.post('/takeBreak/:deviceId', (req: Request, res: Response) => {
        console.log(getSession(req.params.deviceId)?.getSocket)
        getSession(req.params.deviceId)?.getSocket?.write(JSON.stringify({ type: 'break' }));
    })

    const server = app.listen(PORT, () => console.log(`Server running on port ${PORT}`));

    const tcpServer = TcpServer.getInstance();
    tcpServer.init(
        "0.0.0.0",
        9000,
    );
};

main();