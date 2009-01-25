<?php
// $Id$


/**
 * @file
 * This file documents the hooks invoked by the Signup module.
 */

/**
 * Hook invoked when a signup is being canceled.
 *
 * At the time this hook is invoked the record about the signup in the
 * {signup_log} table still exists, but the node has already had its signup
 * total decremented.
 *
 * @param $node
 *   The fully-loaded node object that the signup is being canceled from.
 * @param $signup
 *   An object containing all the known information about the signup being
 *   canceled. Contains all the data from the {signup_log} row representing
 *   the canceled signup. See the schema definition for descriptions of each
 *   field and what they represent.
 *
 * @return
 *   Ignored.
 *
 * @see signup_cancel_signup()
 */
function hook_signup_cancel($signup, $node) {
  $info = array();
  $info[] = t('Signup ID: @sid', array('@sid' => $signup->sid));
  $info[] = t('Node ID: @nid', array('@nid' => $signup->nid));
  $info[] = t('User ID: @uid', array('@uid' => $signup->uid));
  $info[] = t('Email address for anonymous signup: @anon_mail', array('@anon_mail' => $signup->anon_mail));
  $info[] = t('Date/time when the signup was created: @signup_time', array('@signup_time' => $signup->signup_time));
  $form_data = unserialize($signup->form_data);
  $info[] = t('Custom signup form data: %signup_form_data', array('%signup_form_data' => theme('signup_custom_data_email', $form_data)));
  $info[] = t('Attendance record: %attended', array('%attended' => theme('signup_attended_text', $signup->attended)));

  drupal_set_message(theme('item_list', $info, t('Signup canceled for %node_title', array('%node_title' => $node->title))));
}


/**
 * Hook invoked when a signup is being created to gather other signup data.
 *
 * This hook allows other modules to inject information into the custom signup
 * data for each signup.  The array is merged with the values of any custom
 * fields from theme_signup_user_form(), serialized, and stored in the
 * {signup_log} database table.
 *
 * @param $node
 *   Fully-loaded node object being signed up to.
 * @param $account
 *   Full-loaded user object who is signing up.
 *
 * @return
 *   Keyed array of fields to include in the custom data for this signup. The
 *   keys for the array are used as labels when displaying the field, so they
 *   should be human-readable (and wrapped in t() to allow translation).
 *
 * @see signup_sign_up_user()
 * @see theme_signup_user_form()
 */
function hook_signup_sign_up($node, $account) {
  return array(
    t('Node type') => node_get_types('name', $node->type),
    t('User created') => format_date($account->created),
  );
}


/**
 * Hook invoked whenever a node is reopened for signups.
 *
 * A node with signups closed could be reopened in two main cases: 1) someone
 * cancels a signup and the signup limit is no longer reached; 2) a signup
 * administrator manually re-opens signups.
 *
 * @param $node
 *   Fully-loaded node object that is now open for signups.
 *
 * @return
 *   Ignored.
 *
 * @see signup_open_signup()
 */
function hook_signup_open($node) {
  drupal_set_message(t('Duplicate message: signups are now open on %title.', array('%title' => $node->title)));
}


/**
 * Hook invoked whenever a node is closed for signups.
 *
 * Signups are closed in 3 main cases: 1) it is a time-based node and the
 * close-in-advance time has been reached (auto-close via cron); 2) the node
 * has a signup limit and the limit is reached; 3) a signup administrator
 * manually closes signups.
 *
 * @param $node
 *   Fully-loaded node object that is now closed for signups.
 *
 * @return
 *   Ignored.
 *
 * @see signup_close_signup()
 */
function hook_signup_close($node) {
  drupal_set_message(t('Duplicate message: signups are now closed on %title.', array('%title' => $node->title)));
}


/**
 * Hook invoked to see if signup information should be printed for a node.
 *
 * This hook is invoked whenever someone is viewing a signup-enabled node and
 * allows modules to suppress any signup-related output.  If any module's
 * implementation of this hook returns TRUE, no signup information will be
 * printed for that node.
 *
 * @param $node
 *   The fully-loaded node object being viewed.
 *
 * @return
 *   TRUE if you want to prevent signup information from being printed, FALSE
 *   or NULL if the information should be printed.
 *
 * @see _signup_needs_output()
 * @see _signup_menu_access()
 * @see signup_nodeapi()
 */
function hook_signup_suppress($node) {
  if ($node->nid % 2) {
    drupal_set_message(t('Signup information suppressed for odd node ID %nid.', array('%nid' => $node->nid)));
    return TRUE;
  }
}

