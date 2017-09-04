<?php
// $Id: admin.inc,v 1.1 2010/12/08 14:31:47 redijedi Exp $

/**
 * @file
 * Capsule administrative forms.
 */


/**
 * Implementation of hook_admin_settings().
 */
function capsule_admin_settings() {
  $form['capsule_user_token'] = array(
    '#type'          => 'textfield',
    '#title'         => t('User Token'),
    '#default_value' => variable_get('capsule_user_token', ''),
    '#required'      => TRUE,
    '#description'   => t('User token provided by capsule to allow access to the API.'),
  );
  $form['capsule_url_key'] = array(
    '#type'          => 'textfield',
    '#title'         => t('URL Key'),
    '#default_value' => variable_get('capsule_url_key', ''),
    '#required'      => TRUE,
    '#description'   => t('URL key for accessing capsule. Typically in the form http://{URL_KEY}.capsulecrm.com.'),
  );

  return system_settings_form($form);
}

/**
 * Implementation of hook_admin_settings_validate().
 */
function capsule_admin_settings_validate($form, &$form_state) {
  $form_state['values']['capsule_user_token'] = trim($form_state['values']['capsule_user_token']);
  $form_state['values']['capsule_url_key'] = trim($form_state['values']['capsule_url_key']);

  if (empty($form_state['values']['capsule_user_token'])) {
    form_set_error('capsule_user_token', t('User token may not be blank.'));
  }
  if (empty($form_state['values']['capsule_url_key'])) {
    form_set_error('capsule_user_token', t('URL key may not be blank.'));
  }
  else {
    $url = '/party?start=0&limit=1';
    $token = $form_state['values']['capsule_user_token'];
    $key = $form_state['values']['capsule_url_key'];
    try {
      $response = _capsule_http_request($url, 'GET', NULL, $token, $key);
      if ($response->code != '200') {
        throw new Exception($response->status_message);
      }
    } catch (Exception $e) {
      form_set_error('capsule', t('Failed to verify capsule account. Error message is: %error', array(
        '%error' => $e->getMessage()
      )));
    }
  }
}

/**
 * Implementation of hook_admin_settings_submit().
 */
function capsule_admin_settings_submit($form, &$form_state) {
  variable_set('capsule_user_token', $form_state['values']['capsule_user_token']);
  variable_set('capsule_url_key', $form_state['values']['capsule_url_key']);
}

function capsule_api_functions() {
  $header = array(
    t('Function Name'),
  );
  $functions = get_defined_functions();
  foreach ($functions['user'] as $key => $func) {
    $rows[] = array($func);
  }
  return theme('table', array('header'=>$header, 'rows'=>$rows));
}

function get_functions($file) {
  $tokens = token_get_all(file_get_contents("test.php"));
  $comments = array();
  foreach($tokens as $token) {
    if($token[0] == T_COMMENT || $token[0] == T_DOC_COMMENT) {
      $comments[] = $token[1];
    }
  }
  return $comments;
}
