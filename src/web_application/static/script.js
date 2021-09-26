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
