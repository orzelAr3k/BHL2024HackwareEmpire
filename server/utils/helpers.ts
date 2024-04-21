import { customAlphabet } from "nanoid";

export const nid = customAlphabet("1234567890", 5);

export function generateRandIndex(length: number) {
    return Math.floor(Math.random() * length);
}

export const Thresholds = {
    temperatures: {
        min: 20,
        max: 25
    },
    humidity: {
        min: 40,
        max: 60
    }
}