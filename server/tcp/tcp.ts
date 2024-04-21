import * as net from "net";
import * as _ from "lodash";
import { SessionManager } from "./sessionManager";
import { Session } from "./session";

export const sessions: Session[] = [];
export const gamesSessions: GameSessions[] = [];

export function getSession(id: string) {
  return _.find(sessions, { id: id });
}

export function broadcast(message: string, keySelector?: (arg: Session) => boolean, sended?: boolean) {
  _.forEach(keySelector ? _.filter(sessions, keySelector) : sessions, (session: Session) => {
    session.getSocket?.write(message);
    if (sended) {
      session.setSended = sended;
    }
  });
}

export class TcpServer {
  // Singleton ==================================
  private static serverInstance: TcpServer | null = null;


  static getInstance() {
    if (TcpServer.serverInstance == null) {
      TcpServer.serverInstance = new TcpServer();
    }
    return TcpServer.serverInstance;
  }

  constructor() {
    this.server = null;
    this.PORT = 9000;
    this.HOST = "";
  }
  // ============================================

  private sessionManager = new SessionManager();
  private server: net.Server | null;
  private PORT: number;
  private HOST: string;

  init(host: string, port: number) {
    const server = net.createServer((socket: net.Socket) => {
      // on socket connection:
      this.sessionManager.createSession(socket);
    });

    this.server = server;
    this.PORT = port;
    this.HOST = host;
    this.bindServerEvents(server);
  }

  private bindServerEvents(server: net.Server) {
    server.on("error", (err: any) => {
      if (err.code === "EADDRINUSE") {
        console.error("Address in use, retrying...");
        this.retry();
      } else {
        console.error(err.toString());
      }
    });

    server.listen(this.PORT, this.HOST, () => {
      console.log(`Server TCP listen on port: ${this.PORT}`);
    });
  }

  private retry() {
    if (this.server && this.PORT && this.HOST) {
      setTimeout(() => {
        if (this.server) {
          const server = this.server;
          server.close();
          server.listen(this.PORT, this.HOST, () => {
            console.log(`Server TCP listen on port: ${this.PORT}`);
          });
        } else {
          throw Error("Cannot init server!");
        }
      }, 1000);
    }
  }
}