<?php
/**
 * 
{
  "entry" : {
    "party" : {
      "id" : 892
    },
    "type" : "note",
    "content" : "The quick brown fox\njumped over the lazy dog"
  }
}
 * [v2]
 */
function capsule_add_entry($entry = array()) {
  return _capsule_request("/entries", 'POST', $entry);
}
