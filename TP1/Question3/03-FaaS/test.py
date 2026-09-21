import json
import urllib.request


data = {
    "price": 100,
    "discount": 20
}


body = json.dumps(data).encode("utf-8")


request = urllib.request.Request(

    "http://localhost:8083/function",

    data=body,

    headers={
        "Content-Type":
            "application/json"
    },

    method="POST"
)


with urllib.request.urlopen(request) as response:

    print(
        response.read()
        .decode("utf-8")
    )