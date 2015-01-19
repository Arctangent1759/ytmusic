var artist = ""
var title = ""
chrome.runtime.onMessage.addListener(
  function(request, sender, sendResponse) {
    if (request["req"] == "post_vid_info") {
      artist = request["data"]["artist"];
      title = request["data"]["title"];
      sendResponse("success");
    } else if (request["req"] == "get_vid_info"){
      sendResponse({"title":title, "artist":artist});
    }
  });
