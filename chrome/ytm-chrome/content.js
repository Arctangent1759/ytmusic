function post_vid_info() {
  var title = document.getElementById("eow-title").innerHTML.trim();
  var artist = document.getElementsByClassName("yt-user-info")[0].getElementsByTagName("a")[0].innerHTML;
  chrome.extension.sendMessage({"req":"post_vid_info", "data":{"title":title, "artist":artist}}, function(){});
}
window.onfocus = post_vid_info;
post_vid_info();
