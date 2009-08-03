<?php
// $Id$

/**
 * @file
 * Code to confirm the e-mail during signups. Since this code is only needed
 * when building or submitting the signup form (or editing an existing
 * signup), it all lives in this include file and is only loaded when needed.
 */


/**
 * Alter the signup form to add the e-mail confirmation functionality.
 */
function signup_confirm_email_alter_signup_form(&$form, &$form_state, $form_id) {
  global $user;
  unset($_SESSION['signup_email_confirm_sid']);
  unset($_SESSION['signup_email_confirm_values']);
  if (!empty($user->uid) && empty($form['collapse']['signup_user_form']['signup_username'])) {
    // We only need to do this for authenticated users signing up
    // themselves. We already collect the (presumably current) e-mail for
    // "anonymous" signups. If an administrator is signing up another user,
    // there's no reason to include the e-mail confirmation field, either.
    $email_confirm = array(
      '#title' => t('E-mail address'),
      '#type' => 'textfield',
      '#default_value' => $user->mail,
      '#size' => 40,
      '#weight' => -1,
    );
    if ($form_id == 'signup_form') {
      $form['collapse']['email_confirm'] = $email_confirm;
      $form['signup_type'] = array('#type' => 'value', '#value' => 'new');
    }
    elseif ($form_id == 'signup_edit_form') {
      $form['elements']['email_confirm'] = $email_confirm;
      $form['signup_type'] = array('#type' => 'value', '#value' => 'edit');
    }
    $form['#validate'][] = 'signup_email_confirm_validate';
    if (!empty($form['#submit'])) {
      foreach ($form['#submit'] as $key => $callback) {
        if ($callback == 'signup_form_submit') {
          unset($form['#submit'][$key]);
        }
      }
    }
    elseif (!empty($form['elements']['save']['#submit'])) {
      unset($form['elements']['save']['#submit']);
    }
    $form['#submit'][] = 'signup_email_confirm_submit';
  }
}

/**
 * Validation callback for the signup form.
 */
function signup_email_confirm_validate($form, $form_state) {
  if (!empty($form_state['values']['email_confirm']) && 
      !valid_email_address($form_state['values']['email_confirm'])) {
    form_set_error('email_confirm', t('Invalid e-mail address'));
  }
}

/**
 * Submit callback for the signup form.
 *
 * This checks if the e-mail address is being changed. If not, it completes
 * the normal signup form workflow. However, if the e-mail address is changed
 * from the one stored in the profile, the user is redirected to a
 * confirmation form to verify they want to update the e-mail address in their
 * profile before the signup is completed.
 */
function signup_email_confirm_submit($form, &$form_state) {
  global $user;
  if (!empty($form_state['values']['email_confirm'])) {
    if ($form_state['values']['email_confirm'] != $user->mail) {
      if (!empty($form['#signup'])) {
        $_SESSION['signup_email_confirm_sid'] = $form['#signup']->sid;
      }
      $_SESSION['signup_email_confirm_values'] = $form_state['values'];
      if (empty($_SESSION['signup_email_confirm_values']['nid'])) {
        $_SESSION['signup_email_confirm_values']['nid'] = arg(1);
      }
      $nid = $_SESSION['signup_email_confirm_values']['nid'];
      $form_state['redirect'] = "node/$nid/signup/confirm-email";
      return;
    }
  }
  if ($form_state['values']['signup_type'] == 'new') {
    signup_form_submit($form, $form_state);
  }
  else {
    module_load_include('inc', 'signup', 'includes/signup_edit_form');
    signup_edit_form_save_submit($form, $form_state);
  }
}

/**
 * Builds the confirmation form when the user is changing their e-mail address.
 */
function signup_confirm_email_confirm_form() {
  global $user;
  $form['#submit'][] = 'signup_confirm_email_confirm_form_submit';
  $abort_url = isset($_REQUEST['destination']) ? $_REQUEST['destination'] : 'node/' . $_SESSION['signup_email_confirm_values']['nid'];
  $button_text = $_SESSION['signup_email_confirm_values']['signup_type'] == 'new' ? t('Change e-mail address and sign up') : t('Save changes');
  return confirm_form(
    $form,
    t('Are you sure you want to change your e-mail address?'),
    $abort_url,
    t('You have entered a new e-mail address (%new_address) which is different from the one currently saved in your profile (%current_address). If you continue, the new address will be used for future e-mail messages from this site.', array('%new_address' => $_SESSION['signup_email_confirm_values']['email_confirm'], '%current_address' => $user->mail)),
    $button_text, t('Cancel')
  );
}

/**
 * Submit callback for the confirmation form when a user changes their e-mail.
 */
function signup_confirm_email_confirm_form_submit($form, &$form_state) {
  global $user;

  // Update the user's e-mail address in their profile.
  $user->mail = $_SESSION['signup_email_confirm_values']['email_confirm'];
  user_save($user, array('mail' => $user->mail));
  drupal_set_message(t('Updated the e-mail address in your profile to %new_address.', array('%new_address' => $user->mail)));

  // Complete the signup and clear out the SESSION variable.
  $nid = $_SESSION['signup_email_confirm_values']['nid'];
  if ($_SESSION['signup_email_confirm_values']['signup_type'] == 'new') {
    signup_sign_up_user($_SESSION['signup_email_confirm_values']);
  }
  else {
    $signup->sid = $_SESSION['signup_email_confirm_sid'];
    $edit_form['#signup'] = $signup;
    $edit_form_state['values'] = $_SESSION['signup_email_confirm_values'];
    module_load_include('inc', 'signup', 'includes/signup_edit_form');
    signup_edit_form_save_submit($edit_form, $edit_form_state);
  }
  unset($_SESSION['signup_email_confirm_sid']);
  unset($_SESSION['signup_email_confirm_values']);

  // Return to the node they signed up to.
  $form_state['redirect'] = "node/$nid";
}

