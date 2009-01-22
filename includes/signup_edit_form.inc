<?php
// $Id$


/**
 * @file
 * Code for the form to edit existing signups.
 */

/**
 * Build the form for editing existing signups.
 *
 * @param $form_state
 *   The state of the form to build (not currently used).
 * @param $signup
 *   The fully-loaded signup object with data about the signup to edit.
 * @param $type
 *   The type of signup edit form to render, can be 'node', 'tab', or 'admin'.
 *
 * @return
 *   The FAPI form array for the signup edit form.
 */
function signup_edit_form($form_state, $signup, $type) {
  global $user;
  $form = array();

  $form['#signup'] = $signup;
  $node = node_load($signup->nid);

  // Check permissions.
  $admin = _signup_menu_access($node, 'admin');
  $own = !empty($user->uid) && $user->uid == $signup->uid;
  $can_cancel = $admin || (user_access('cancel own signups') && $own);
  $can_edit = $admin || (user_access('edit own signups') && $own);

  if ($type == 'admin') {
    $title = t("Information for !user's signup to !node", array('!user' => _signup_get_username($signup, TRUE), '!node' => l($node->title, 'node/'. $node->nid)));
  }
  else {
    $title = t('Your signup information');
  }

  if ($type == 'node') {
    $form['elements'] = array(
      '#type' => 'fieldset',
      '#title' => $title,
      '#collapsible' => TRUE,
      '#collapsed' => variable_get('signup_fieldset_collapsed', 1),
    );
  }
  else {
    $form['elements'] = array();
    $form['elements']['header'] = array(
      '#type' => 'markup',
      '#value' => $title,
      '#prefix' => '<h4>',
      '#suffix' => '</h4>',
    );
  }

  if (!empty($signup->anon_mail)) {
    $form['elements']['signup_anon_mail'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => $signup->anon_mail,
      '#size' => 40,
      '#maxlength' => 255,
      '#required' => TRUE,
    );
    $form['#validate'][] = 'signup_validate_anon_mail';
  }

  if ($type == 'admin') {
    $options = array();
    if (1|| !isset($signup->attended)) {
      $options[-1] = t('- Not recorded -');
    }
    $options[1] = theme('signup_attended_text', 1);
    $options[0] = theme('signup_attended_text', 0);
    $form['elements']['attended'] = array(
      '#type' => 'select',
      '#title' => t('Attendance'),
      '#default_value' => isset($signup->attended) ? $signup->attended : -1,
      '#options' => $options
    );
  }

  // Build the themed signup form for this site and include that.
  $site_form = theme('signup_user_form', $node);
  $form_data = unserialize($signup->form_data);

  // This is sort of a hack, but we don't support nested arrays for the custom
  // signup form anyway, so it works for now.  Obviously all this will change
  // with signup_fields and friends, but for now it works.
  foreach ($form_data as $key => $value) {
    if (!empty($site_form['signup_form_data'][$key])) {
      $site_form['signup_form_data'][$key]['#default_value'] = $value;
      if (!$can_edit) {
        // If they can't edit, mark all the fields as disabled.
        $site_form['signup_form_data'][$key]['#disabled'] = TRUE;
        // Also, remove the required attribute from all fields (so that the
        // cancel button, if it exists, will work).
        $site_form['signup_form_data'][$key]['#required'] = FALSE;
      }
    }
  }
  $form['elements'] += $site_form;

  // Add the appropriate buttons based on permissions.
  if ($can_edit) {
    $form['elements']['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('signup_edit_form_save_submit'),
    );
  }
  if ($can_cancel) {
    $form['elements']['cancel-signup'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel signup'),
      '#submit' => array('signup_edit_form_cancel_submit'),
    );
  }

  return $form;
}

/**
 * Validation callback when editing the anonymous email for an existing signup.
 */
function signup_validate_anon_mail($form, &$form_state) {
  $mail = $form_state['values']['signup_anon_mail'];
  if (!valid_email_address($mail)) {
    form_set_error('signup_anon_mail', t('The e-mail address %mail is not valid.', array('%mail' => $mail)));
  }
}

/**
 * Submit callback when saving changes to an existing signup.
 */
function signup_edit_form_save_submit($form, $form_state) {
  $signup_info = array();
  if (!empty($form_state['values']['signup_form_data'])) {
    $signup_info = $form_state['values']['signup_form_data'];
  }
  $values[] = "form_data = '%s'";
  $args[] = serialize($signup_info);

  // If the form contains an e-mail address for an anonymous signup, save it.
  if (!empty($form_state['values']['signup_anon_mail'])) {
    $values[] = "anon_mail = '%s'";
    $args[] = $form_state['values']['signup_anon_mail'];
  }

  // If the form contains attendance info, save it.
  if (isset($form_state['values']['attended'])) {
    if ($form_state['values']['attended'] == -1) {
      $values[] = "attended = NULL";
    }
    else {
      $values[] = "attended = %d";
      $args[] = $form_state['values']['attended'];
    }
  }

  /// @TODO invoke a hook?
  /// @TODO should we have a "signup_update_time" field and update that, too?
  $updates = implode(', ', $values);
  $args[] = $form['#signup']->sid;
  db_query("UPDATE {signup_log} SET $updates WHERE sid = %d", $args);
  drupal_set_message(t('Signup information updated.'));
}

/**
 * Submit callback to cancel the signup when editing an existing signup.
 */
function signup_edit_form_cancel_submit($form, &$form_state) {
  signup_cancel_signup($form['#signup']->sid);
  if (empty($_REQUEST['destination'])) {
    // If there's no destination already set, redirect to the node.
    $form_state['redirect'] = 'node/'. $form['#signup']->nid;
  }
}

/**
 * Page handler for the administrator page to edit an existing signup.
 *
 * @param $signup
 *   The fully-loaded signup object to edit.
 *
 * @return
 *   The HTML to use for the signup edit page.
 */
function signup_edit_page($signup) {
  drupal_set_title(t('Edit signup'));
  return drupal_get_form('signup_edit_form', $signup, 'admin');
}

