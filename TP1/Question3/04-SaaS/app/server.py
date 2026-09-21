from http.server import BaseHTTPRequestHandler, HTTPServer

HOST = "0.0.0.0"
PORT = 8084


HTML = """
<!DOCTYPE html>

<html lang="fr">

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<title>Promo Calculator</title>

<style>

body {

    font-family: Arial;

    max-width: 600px;

    margin: 60px auto;

    padding: 20px;

    background: #f4f4f4;
}


.card {

    background: white;

    padding: 30px;

    border-radius: 12px;

}


input,
button {

    display: block;

    width: 100%;

    box-sizing: border-box;

    padding: 12px;

    margin: 10px 0 20px;

    font-size: 16px;

}


button {

    cursor: pointer;

}


#result {

    font-size: 24px;

    font-weight: bold;

}

</style>

</head>


<body>

<div class="card">

<h1>Promo Calculator</h1>

<p>
Application SaaS de calcul de promotions
</p>


<label>
Prix original
</label>

<input
    id="price"
    type="number"
    value="100"
    min="0"
/>


<label>
Réduction (%)
</label>

<input
    id="discount"
    type="number"
    value="20"
    min="0"
    max="100"
/>


<button onclick="calculate()">
Calculer
</button>


<div id="result"></div>


</div>


<script>

function calculate() {

    const price =
        Number(
            document.getElementById(
                "price"
            ).value
        );


    const discount =
        Number(
            document.getElementById(
                "discount"
            ).value
        );


    if (
        price < 0 ||
        discount < 0 ||
        discount > 100
    ) {

        document.getElementById(
            "result"
        ).textContent =
            "Valeurs invalides";

        return;
    }


    const finalPrice =
        price *
        (1 - discount / 100);


    document.getElementById(
        "result"
    ).textContent =
        "Prix final : $" +
        finalPrice.toFixed(2);

}

</script>

</body>

</html>
"""


class SaaSHandler(BaseHTTPRequestHandler):

    def do_GET(self):

        if self.path != "/":

            self.send_response(404)

            self.end_headers()

            return


        body = HTML.encode("utf-8")


        self.send_response(200)


        self.send_header(
            "Content-Type",
            "text/html; charset=utf-8"
        )


        self.send_header(
            "Content-Length",
            str(len(body))
        )


        self.end_headers()


        self.wfile.write(body)


if __name__ == "__main__":

    print(
        "SaaS application running on "
        "http://localhost:8084"
    )


    HTTPServer(
        (HOST, PORT),
        SaaSHandler
    ).serve_forever()