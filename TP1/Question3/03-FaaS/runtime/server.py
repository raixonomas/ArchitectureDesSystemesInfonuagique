from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

sys.path.insert(
    0,
    str(ROOT / "function")
)

from promo_function import calculate_price


HOST = "0.0.0.0"
PORT = 8083


class FunctionHandler(BaseHTTPRequestHandler):

    def send_json(self, status, data):

        body = json.dumps(data).encode("utf-8")

        self.send_response(status)

        self.send_header(
            "Content-Type",
            "application/json"
        )

        self.send_header(
            "Content-Length",
            str(len(body))
        )

        self.end_headers()

        self.wfile.write(body)

    def do_POST(self):

        if self.path != "/function":

            self.send_json(
                404,
                {"error": "Use POST /function"}
            )

            return

        try:

            length = int(
                self.headers.get(
                    "Content-Length",
                    "0"
                )
            )

            body = self.rfile.read(length)

            event = json.loads(body)

            result = calculate_price(event)

            self.send_json(
                200,
                result
            )

        except (
            ValueError,
            KeyError,
            json.JSONDecodeError
        ) as exception:

            self.send_json(
                400,
                {
                    "error": str(exception)
                }
            )


if __name__ == "__main__":

    print(
        "FaaS runtime running on "
        "http://localhost:8083"
    )

    HTTPServer(
        (HOST, PORT),
        FunctionHandler
    ).serve_forever()