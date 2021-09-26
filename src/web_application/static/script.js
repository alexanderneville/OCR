function delete_model(model_id, user_id) {
    console.log("delete model function");

    var request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "_delete_model", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";

    data = {"model_id": model_id, "user_id": user_id};
    console.log(data);

    request.onload = function(){

        var responseJson = request.response;

        console.log(responseJson);
        console.log(responseJson.status);

        if (responseJson['status'] == 1) {

            let div_id = "model";
            div_id = div_id.concat(model_id.toString());

            console.log(div_id);
            document.getElementById(div_id).remove();

        }

    }

    request.send(JSON.stringify(data));

}

function leave_class(class_id, user_id) {

    console.log("leave class function");

    var request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "_leave_class", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";

    data = {"class_id": class_id, "user_id": user_id};
    console.log(data);

    request.onload = function(){

        var responseJson = request.response;

        console.log(responseJson);
        console.log(responseJson.status);

        if (responseJson['status'] == 1) {

            let div_id = "class";
            div_id = div_id.concat(class_id.toString());

            console.log(div_id);
            document.getElementById(div_id).remove();

        }

    }

    request.send(JSON.stringify(data));

}

function delete_class(class_id, user_id) {
    console.log("delete class function");

    var request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "_delete_class", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";

    data = {"class_id": class_id, "user_id": user_id};
    console.log(data);

    request.onload = function(){

        var responseJson = request.response;

        console.log(responseJson);
        console.log(responseJson.status);

        if (responseJson['status'] == 1) {

            let div_id = "class"
            div_id = div_id.concat(class_id.toString())

            console.log(div_id);
            document.getElementById(div_id).remove();

        }

    }

    request.send(JSON.stringify(data));
}

function kick_student(student_id, class_id, user_id) {

    console.log("leave class function");

    var request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "_kick_student", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";

    data = {"student_id": student_id, "class_id": class_id, "user_id": user_id};
    console.log(data);

    request.onload = function(){

        var responseJson = request.response;

        console.log(responseJson);
        console.log(responseJson.status);

        if (responseJson['status'] == 1) {

            let div_id = class_id.toString();
            div_id = div_id.concat("student");
            div_id = div_id.concat(student_id.toString())

            console.log(div_id);
            document.getElementById(div_id).remove();

        }

    }

    request.send(JSON.stringify(data));
}

function delete_cache(cache_id, user_id) {

    console.log("delete cache function");

    var request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "_delete_cache", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";

    data = {"cache_id": cache_id, "user_id": user_id};
    console.log(data);

    request.onload = function(){

        var responseJson = request.response;

        console.log(responseJson);
        console.log(responseJson.status);

        if (responseJson['status'] == 1) {

            let div_id = "cache";
            div_id = div_id.concat(cache_id.toString());

            console.log(div_id);
            document.getElementById(div_id).remove();

        }

    }

    request.send(JSON.stringify(data));

}

function rgb_to_hex(r,g,b) {

    r = r.toString(16);
    g = g.toString(16);
    b = b.toString(16);

    if (r.length == 1)
        r = "0" + r;
    if (g.length == 1)
        g = "0" + g;
    if (b.length == 1)
        b = "0" + b;

    return "#" + r + g + b;

}

function draw_matrix(character) {

    let canvas = document.getElementById("matrix_canvas");
    let cursor = canvas.getContext("2d");
    for (let y = 0; y < 32; y++) {
        for (let x = 0; x < 32; x++)  {
            let greyscale = Math.floor(character[y][x]);
            let hex = rgb_to_hex(greyscale, greyscale, greyscale);
            console.log(hex);
            cursor.fillStyle = hex;
            cursor.fillRect(x*10, y*10, 10, 10);
        }
    }
}

function get_first_character(model_id, user_id){

    console.log("get first character function");

    var request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "train_model", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";

    data = {"model_id": model_id, "user_id": user_id};
    console.log(data);

    request.onload = function() {

        var responseJson = request.response;

        if (responseJson["complete"] == 1) {
            // go home
        } else {

            let new_character = responseJson["new_character"];
            draw_matrix(new_character);

        }

    }

    request.send(JSON.stringify(data));
 
}

function label_character(model_id, user_id) {

    console.log("label character function");

    let request = new XMLHttpRequest();   // new HttpRequest instance
    request.open("POST", "train_model", true);
    request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    request.responseType = "json";
    let label = document.getElementById('label').value;
    document.getElementById('label').value = "";
    console.log(label);
    let data = {"model_id": model_id, "user_id": user_id, "label": label};
    console.log(data);

    request.onload = function() {

        var responseJson = request.response;

        if (responseJson["complete"] == 1 || responseJson["status"] == 0) {
            window.location.href="/home";
        } else {

            let new_character = responseJson["new_character"];
            draw_matrix(new_character);

        }
    }

    request.send(JSON.stringify(data));

}

function toggle_students(classname) {
    console.log(classname);
    var elements = document.getElementsByClassName(classname);

    for(var i = 0; i < elements.length; i++){
        if (elements[i].style.display == "none") {
             elements[i].style.display = "block";
        } else {
            elements[i].style.display = "none";
        }
    }

}
