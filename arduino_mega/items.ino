void request_items(int itemId) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    // data that will be send
    root["type"]    = "REQUEST_ITEM";
    root["id"]      = itemId;

    root.printTo(Middleware);   // send data to nodemcu
    root.prettyPrintTo(Serial); // preview request in serial
}

void get_items() {
    if (Middleware.available() > 0) {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(Middleware);

        if (root.success()) {
            // store recieved data to global variables
            Items.id    = root["id"];
            Items.name  = root["name"].as<String>();
            Items.price = root["price"];
            Items.qty   = root["qty"];
            Items.selected = true;

            display_items();            // display item info in lcd
            root.prettyPrintTo(Serial); // preview received data in serial
        }
    }
}

void purchase_items() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    // get item to be purchase
    root["type"]    = "PURCHASE_ITEM";
    root["id"]      = Items.id;
    root["name"]    = Items.name;
    root["price"]   = Items.price;

    clear_items();

    root.printTo(Middleware);   // send data to nodemcu
    root.prettyPrintTo(Serial); // preview data to be send in serial
}

void clear_items() {
    Items.id        = 0;
    Items.name      = "";
    Items.price     = 0;
    Items.qty       = 0;
    Items.selected  = false;
}
