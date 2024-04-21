interface Measurement {
    date: Data;
    temperature: number;
    noise: string;
    humidity: number;
}

interface Station {
    _id: number;
    devices: string[];
}

// interface Device {
//     _id: string;
//     stationId: string;
//     username: string;
//     busy: boolean;
// }

interface Game {
    _id: string;
    name: string;
    members: number;
}

interface Message {
    type: 'id' | 'accept' | 'reject' | 'busy';
    deviceId: string;
    gameId?: string;
}

interface GameSessions {
    id: string;
    maintainer: string;
    players: {
        id: string;
        status: 'accept' | 'reject';
        session: Session;
    }[];
    game: Game;
}