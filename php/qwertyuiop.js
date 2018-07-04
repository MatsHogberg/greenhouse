$(document).ready(function(){
    doIt();
});

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
    
}
function noResponseFromController(){
    
}