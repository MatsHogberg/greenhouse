var controllerBaseUrl ="http://xxx.xx.xxx.xxx:xx";
var status = "/status";
var control = "/flip";
var pumpOk = false;

$(document).ready(function(){
    doIt();
});
function doIt(){
    $("#bigButton").on("click", function(){
        flipStatus();
    });
    $.ajax({
        type: "GET",
        url: controllerBaseUrl + status,
        success: function(result){
            gotStatusResponse(result);
        },
        error: function(){
            noResponseFromController();
        }
    });
}
function gotStatusResponse(s){
    pumpOk = true;
    $("#errorMsg").text("");
    $("#buttonLabel").text(s==1?"Stäng av pumpen":"Sätt på pumpen");
}
function noResponseFromController(){
    $("#errorMsg").text("Något gick fel när pumpen tillfrågades.");
    $("#buttonLabel").text("");
}
function flipStatus(){
    if(pumpOk){
        $.ajax({
            type: "GET",
            url: controllerBaseUrl + control,
            success: function(result){
                gotStatusResponse(result)
            },
            error: function(){
                noResponseFromController();
            }
        });
    }else{
        $("#errorMsg").innerText("Kan inte styra pumpen just nu. Ladda om sidan och försök igen.");
    }
}

// 176.10.142.121
