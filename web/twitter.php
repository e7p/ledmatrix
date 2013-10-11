<?php
header('Content-Type: text/plain');

function buildBaseString($baseURI, $method, $params) { 
  $r = array();
  ksort($params);
  foreach($params as $key=>$value) { 
    $r[] = "$key=" . rawurlencode($value);
  } 
  return $method."&" . rawurlencode($baseURI) . '&' . rawurlencode(implode('&', $r));
}

function buildAuthorizationHeader($oauth) { 
  $r = 'Authorization: OAuth ';
  $values = array();
  foreach($oauth as $key=>$value) { 
    $values[] = "$key=\"" . rawurlencode($value) . "\""; 
  }
  $r .= implode(', ', $values);
  return $r;
}

function getTweets() {
  $id = 0;
  if(isset($_GET['id'])) $id = intval($_GET['id']);
  $url = "https://api.twitter.com/1.1/search/tweets.json";
  $data = array(
    'q' => '@devtal',
    //'lang' => 'de',
    'result_type' => 'recent',
    'include_entities' => 'true',
    'since_id' => $id
  );
  $oauth_access_token = "97934335-KNVAE9FexAmMiGN20lr0iP4hHW6CcyKp0xzMoV8qp";
  $oauth_access_token_secret = "WMRwWFhT9LNWX0ZWHbb9nuvraoLsGbC4D34eMbG6eg";
  $consumer_key = "Hr842nn0zXOsgywiFDHbA";
  $consumer_secret = "B79eYSuylQGEr0W494cOE5UWiJEbjsV9OtgIDZuQNH8";
  $oauth = array(
    'oauth_consumer_key' => $consumer_key,
    'oauth_nonce' => time(),
    'oauth_signature_method' => 'HMAC-SHA1',
    'oauth_token' => $oauth_access_token,
    'oauth_timestamp' => time(),
    'oauth_version' => '1.0'
  );
  $oauth = array_merge($oauth, $data);
  $base_info = buildBaseString($url, 'GET', $oauth);
  $composite_key = rawurlencode($consumer_secret) . '&' . rawurlencode($oauth_access_token_secret);
  $oauth_signature = base64_encode(hash_hmac('sha1', $base_info, $composite_key, true));
  $oauth['oauth_signature'] = $oauth_signature;

  // Make Requests
  $data_string = '';
  ksort($data);
  foreach($data as $key=>$value) { 
    $data_string .= "&$key=" . rawurlencode($value);
  }
  $data_string[0] = '?';
  
  $header = array(buildAuthorizationHeader($oauth), 'Expect:');
  $options = array( CURLOPT_HTTPHEADER => $header, //CURLOPT_POSTFIELDS => $postfields,
    CURLOPT_HEADER => false,
    CURLOPT_URL => $url . $data_string,
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_SSL_VERIFYPEER => false
  );
  $feed = curl_init();
  curl_setopt_array($feed, $options);
  $json = curl_exec($feed);
  curl_close($feed);
  return json_decode($json);
}

$tweets = getTweets();
foreach($tweets->statuses as $tweet) {
  echo $tweet->id."\n";
  echo '@'.$tweet->user->screen_name.': '.$tweet->text;
  //echo '/dev/täl Häckerspäce Wüppertäl!';
  exit;
}
?>
