from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs
import json
import os

HOST = "0.0.0.0"
PORT = int(os.environ.get("PORT", "8082"))


def calculate_price(price, discount):
    return round(price * (1 - discount / 100), 2)


class PromoHandler(BaseHTTPRequestHandler):

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

    def do_GET(self):

        parsed = urlparse(self.path)

        if parsed.path != "/promo":
            self.send_json(
                404,
                {"error": "Route not found"}
            )
            return

        params = parse_qs(parsed.query)

        try:

            price = float(params["price"][0])
            discount = float(params["discount"][0])

            if price < 0 or discount < 0 or discount > 100:
                raise ValueError

            self.send_json(
                200,
                {
                    "model": "PaaS",
                    "originalPrice": price,
                    "discount": discount,
                    "finalPrice":
                        calculate_price(
                            price,
                            discount
                        )
                }
            )

        except (KeyError, ValueError):

            self.send_json(
                400,
                {
                    "error":
                        "Use /promo?price=100&discount=20"
                }
            )


if __name__ == "__main__":

    print(
        "PaaS Promo API running on "
        "http://localhost:8082"
    )

    HTTPServer(
        (HOST, PORT),
        PromoHandler
    ).serve_forever()