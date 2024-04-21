import * as net from "net";
import { broadcast, gamesSessions } from "./tcp";

export class Session {
	private id: string = "";
	private socket: net.Socket | null;
	private busy: boolean = false;
	private sended: boolean = false;

	constructor(socket: net.Socket) {
		this.socket = socket;
		this.bindSocketEvents(socket);
	}

	get getId() {
		return this.id;
	}

	get getSocket() {
		return this.socket;
	}

	get getIp() {
		return this.socket?.remoteAddress || "";
	}

	get getBusy() {
		return this.busy;
	}

	set setSended(value: boolean) {
		this.sended = value;
	}

	get getSended() {
		return this.sended;
	}

	dispose() {
		this.socket = null;
	}

	private bindSocketEvents(socket: net.Socket) {
		socket.on("data", (data: Buffer) => {
			const message = data.toString("utf-8").trim();
			console.log(this.getIp + " : " + message);
			this.handleAction(this, message);
		});

		socket.on("error", (err: any) => {
			console.error(err.toString());
		});

		socket.on("close", () => {
			console.log(`Client disconnected ${this.getIp} : ${this.id}`)
		});
	}

	handleAction(session: Session, message: string) {
		const json: Message = JSON.parse(message);
		console.log(json);
		switch (json.type) {
			case 'id': {
				this.id = json.deviceId;
				console.log(this.id);
				break;
			}
			case 'accept': {
				const game = gamesSessions.find(g => g.id === json.gameId);
				if (game && game.players.length < game.game.members - 1) {
					game.players.push({ id: json.deviceId, status: json.type, session: this });
					this.socket?.write(JSON.stringify({ status: 'accepted' }));
					this.sended = true;
					broadcast(JSON.stringify({
						type: 'game',
						gameId: game.id,
						name: game.game.name,
						members: game.game.members,
						players: game.players.filter(p => p.status === 'accept').length + 1
					}), (s: Session) => !s.getBusy)
				}
				if (game && game?.players.filter(p => p.status === 'accept').length === game.game.members - 1) {
					game.players.forEach(p => p.session.setSended = false);
					const gameIndex = gamesSessions.findIndex(g => g.id === json.gameId);
					gamesSessions.splice(gameIndex, 1);
					broadcast(JSON.stringify({ type: 'game_accept', gameId: game.id, status: 'ended' }), (s: Session) => !s.getBusy, false)
				}
				console.log(game);
				break;
			}
			case 'reject': {
				const game = gamesSessions.find(g => g.id === json.gameId);
				if (game && game.players.length < game.game.members - 1) {
					game.players.push({ id: json.deviceId, status: json.type, session: this });
					this.socket?.write(JSON.stringify({ type: 'game_response', status: 'ack' }));
					this.sended = false;
				}
				break;
			}
			case 'busy': {
				this.busy = !this.busy;
			}
		}
	}
}