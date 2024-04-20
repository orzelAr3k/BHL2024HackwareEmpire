import express, { Request, Response } from 'express';

import "dotenv/config";

const PORT = process.env.PORT || 3000;
const app = express();



app.get('/', (req: Request, res: Response) => {
    res.status(200).json({ test: 'test' });
})





app.listen(PORT, () => console.log(`Server running on port ${PORT}`));