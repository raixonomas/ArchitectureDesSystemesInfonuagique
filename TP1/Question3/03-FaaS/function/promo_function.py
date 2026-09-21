def calculate_price(event):

    price = float(event["price"])
    discount = float(event["discount"])

    if price < 0:
        raise ValueError(
            "Price must be greater than or equal to 0"
        )

    if discount < 0 or discount > 100:
        raise ValueError(
            "Discount must be between 0 and 100"
        )

    final_price = round(
        price * (1 - discount / 100),
        2
    )

    return {
        "model": "FaaS",
        "originalPrice": price,
        "discount": discount,
        "finalPrice": final_price
    }