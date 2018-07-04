$(document).ready(function(){
    doIt();
});
var pumpOk = false;
function doIt(){
    var controllerBaseUrl ="http://haga.tplinkdns.com:8084";
    var status = "/status";
    var control = "/flip";
    $.ajax({
        type: GET,
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
    $("#errorMsg").innerText("");
    $("#buttonLabel").innerText(s==1?"Stäng av pumpen":"Sätt på pumpen");
}
function noResponseFromController(){
    $("#errorMsg").innerText("Något gick fel när pumpen tillfrågades.");
    $("#buttonLabel").innerText("");
}
function flipStatus(){
    if(pumpOk){
        $.ajax({
            type: GET,
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