import * as net from "net";
import * as _ from "lodash";
import { customAlphabet } from "nanoid";
import { Session } from "./session";
import { sessions } from "./tcp";

export class SessionManager {
	// private sessions: Session[] = [];

	createSession(socket: net.Socket) {
		// const nid = customAlphabet("1234567890", 5);
		// const id = nid();
		const session = new Session(socket);
		sessions.push(session);
        console.log(session.getIp);
	}
}