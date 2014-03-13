//
//  Created by Erica Chang on 3/12/2014.
//  Copyright (c) 2014 Woozilli, Inc. All rights reserved.
//

var pebbleToken;
// -- Ready state event listener --
Pebble.addEventListener("ready", function(e) {
        pebbleToken = Pebble.getAccountToken();
        console.log("Pebble Account Token: " + pebbleToken);
    }
);

// -- Configuration --
Pebble.addEventListener("showConfiguration", function() {
    Pebble.openURL('https://www.woozilli.com/pebble?pebble_token=' + encodeURIComponent(pebbleToken));
});

// -- Configuration web view closed --
Pebble.addEventListener("webviewclosed", function(e) {
    if (e && e.response) {
        var options = JSON.parse(e.response);
        console.log("Options = " + JSON.stringify(options));
    }
});

// -- Inbound appmessage --
Pebble.addEventListener("appmessage", function(e) {
    console.log("message:" + JSON.stringify(e.payload));
    if (e.payload.hasOwnProperty("fetch_item")) {
        fetchTodoList();
    } else if (e.payload.hasOwnProperty("delete_item")) {
        deleteTodoItem(e.payload.delete_item);
    }
});

// Woozilli Cloud Platform API request parameters
var WZ_API_KEY = "YOUR WOOZILLI API KEY";
var WZ_APP_ID = "YOUR WOOZILLI PEBBLE TODO APP ID";
var WZ_OBJ_NAME = "YOUR TODO LIST OBJECT NAME";
var WZ_OBJ_ID = "YOUR TODO LIST OBJECT ID";
var WZ_REQ_TYPE_READ = "read";
var WZ_REQ_TYPE_DELETE = "delete";

// POST web requests to Woozilli Cloud Platform 
function sendWebRequest(reqBody, callback){
    var xhr = new XMLHttpRequest();
    var err = null;
    xhr.open("POST", "https://api.woozilli.com/apiengine", true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.setRequestHeader("Content-Length", reqBody.length);
    xhr.onreadystatechange = function() {
        // readyState 4 DONE
        if (xhr.readyState == 4 && xhr.status == 200) {
            console.log(xhr.responseText);
            var jsonResp;
            if (xhr.responseText) {
                jsonResp = JSON.parse(xhr.responseText);
                if (jsonResp && jsonResp.details){
                    callback(err, jsonResp.details);
                } else {
                    err = new Error(500, "Response error");
                    callback(err);
                }
            } else {
                err = new Error(500, "Server error");
                callback(err);
            }
        } else {
            err = new Error(xhr.status, xhr.statusText);
            callback(err);
        }
    }
    xhr.send(reqBody);
}

// Construct Woozilli Cloud Platform "read" API request to fetch todo list items
function fetchTodoList(){
    var jsonReq = {
            api_key: WZ_API_KEY,
            app_id: WZ_APP_ID,
            obj_name: WZ_OBJ_NAME,
            obj_id: WZ_OBJ_ID,
            type: WZ_REQ_TYPE_READ,
            cols_ops_vals:[
            {
                column: "Todo ID",
                value:-1,
                operator: ">"
            },
            {
                column: "Pebble Token",
                value: pebbleToken,
                operator: "="
            }
            ],
            and_or: [{"and_or": "and"}]
    };
    var reqBody = JSON.stringify(jsonReq);
    sendWebRequest(reqBody, function(err, results){
        if (results) {
            var i = 0;
            function sendMsg() {
                if (i < results.length) {
                    Pebble.sendAppMessage({
                        "append_text": results[i]["Todo Item"].toString(),
                        "append_id": results[i]["Todo ID"]}, function(e){
                            console.log("Successfully delivered results " + i);
                            i++;
                            sendMsg();
                        });
                }
            };
            sendMsg();
        }
    });
}

// Construct Woozilli Cloud Platform "delete" API request to delete todo list item based on unique primary key
function deleteTodoItem(itemId){
    console.log("deleteTodoItem" + itemId);
    var jsonReq = {
            api_key: WZ_API_KEY,
            app_id: WZ_APP_ID,
            obj_name: WZ_OBJ_NAME,
            obj_id: WZ_OBJ_ID,
            type: WZ_REQ_TYPE_DELETE,
            cols_ops_vals:[
            {
                column: "Todo ID",
                value: itemId,
                operator: "="
            }],
            and_or: []
    };
    var reqBody = JSON.stringify(jsonReq);
    sendWebRequest(reqBody, function(err, results){
        
    });
}
