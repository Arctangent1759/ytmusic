var port = 1759;
var r = RegExp("^https?://www\\.youtube\\.com/watch\\?v=([a-zA-Z0-9_-]{11}).*$");

function getCurrentTabUrl(callback) {
  var queryInfo = {
    active: true,
    currentWindow: true
  };
  chrome.tabs.query(queryInfo, function(tabs) {
    var tab = tabs[0];
    var url = tab.url;
    console.assert(typeof url == 'string', 'tab.url should be a string');
    callback(url);
  });
}

/*
var title = document.getElementById("eow-title").innerHTML.trim();
var artist = document.getElementsByClassName("yt-user-info")[0].getElementsByTagName("a")[0].innerHTML;
chrome.extension.sendMessage({'title':document.getElementById('eow-title').innerHTML.trim(), 'artist':document.getElementsByClassName('yt-user-info')[0].getElementsByTagName('a')[0].innerHTML}, function(){});

chrome.runtime.onMessage.addListener(
  function(request, sender, sendResponse) {
    artist = request["artist"];
    title = request["title"];
  });

document.addEventListener('DOMContentLoaded', function() {
    getCurrentTabUrl(function(url) {
      if (r.exec(url)) {
        var youtube_hash = r.exec(url)[1];
        chrome.extension.sendMessage({"req":"get_vid_info"},function(response){
          document.getElementById("youtube_hash").value = youtube_hash;
          document.getElementById("title").value = response["title"];
          document.getElementById("artist").value = response["artist"];
        });
      }
    });
    document.getElementById("submit_button").onclick = function() {
      var title = document.getElementById("title").value;
      var yt_hash = document.getElementById("youtube_hash").value;
      var artist = document.getElementById("artist").value;
      var album = document.getElementById("album").value;
      var ytm_request = 'http://localhost:'+port+'/AddSong("'+title+'", "'+yt_hash+'", "'+artist+'", "'+album+'")\n';
      var r = new XMLHttpRequest();
      r.open("GET",ytm_request);
      r.send();
    }
});
*/

document.addEventListener('DOMContentLoaded', function() {


  getCurrentTabUrl(function(url) {
      if (r.exec(url)) {
        var youtube_hash = r.exec(url)[1];

        chrome.runtime.onMessage.addListener(
          function(request, sender, sendResponse) {
            document.getElementById("youtube_hash").value = youtube_hash;
            document.getElementById("title").value = request["title"];
            document.getElementById("artist").value = request["artist"];
          });

        chrome.tabs.getSelected(null, function(tab) {
          chrome.tabs.executeScript(tab.id, {
              code: "chrome.extension.sendMessage({'title':document.getElementById('eow-title').innerHTML.trim(), 'artist':document.getElementsByClassName('yt-user-info')[0].getElementsByTagName('a')[0].innerHTML}, function(){});"
             }, function() {});
        });

      }
    });
    document.getElementById("submit_button").onclick = function() {
      var title = document.getElementById("title").value;
      var yt_hash = document.getElementById("youtube_hash").value;
      var artist = document.getElementById("artist").value;
      var album = document.getElementById("album").value;
      var ytm_request = 'http://localhost:'+port+'/AddSong("'+title+'", "'+yt_hash+'", "'+artist+'", "'+album+'")\n';
      var r = new XMLHttpRequest();
      r.open("GET",ytm_request);
      r.send();
    }
});

