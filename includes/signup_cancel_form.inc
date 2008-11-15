<?php
// $Id$


/**
 * @file
 * Code for the signup cancel form.
 */

/**
 * Submit handler for the cancel signup form.
 *
 * @param $form
 *   The form being submitted.
 * @param $form_state
 *   The state of the submitted form, including the values.
 */
function signup_cancel_form_submit($form, &$form_state) {
  $uid = $form_state['values']['uid'];
  $nid = $form_state['values']['nid'];
  $anon_mail = NULL;
  if (isset($form_state['values']['signup_anon_mail'])) {
    $anon_mail = $form_state['values']['signup_anon_mail'];
  }
  signup_cancel_signup($uid, $nid, $anon_mail);
}

/**
 * Form builder for the cancel signup form.
 */
function signup_cancel_form(&$form_state, $node, $account) {
  $form['nid'] = array('#type' => 'value', '#value' => $node->nid);
  $form['uid'] = array('#type' => 'value', '#value' => $account->uid);
  if (!empty($account->anon_mail)) {
    $form['signup_anon_mail'] = array('#type' => 'value', '#value' => $account->anon_mail);
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Cancel signup'));
  return $form;
}

