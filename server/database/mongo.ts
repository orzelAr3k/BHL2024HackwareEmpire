import { MongoClient } from 'mongodb';

import 'dotenv/config';

export async function connectionToMongo() {
    const client = new MongoClient(process.env.MONGO_URL || 'mongodb://localhost:27017');
    await client.connect();
    return client.db('BHL');
}

