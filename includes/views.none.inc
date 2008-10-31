<?php
// $Id$

/**
 * @file
 * Provides all the code for required UI elements for sites that do
 * not have views.module enabled. If views is enabled, there are
 * default views for all of these things (which are therefore
 * customizable and more powerful) in signup/views/views.inc.
 *
 */

/**
 * Implementation of hook_block().
 * @ingroup signup_core
 * @param $op
 *   The operation that is being requested. This defaults to 'list', which
 *   indicates that the method should return which blocks are available.
 * @param $delta
 *   The specific block to display (the offset into an array).
 * @return
 *   One of two possibilities. The first is an array of available blocks.
 *   The other is an array containing a block.
 */
function signup_block($op = 'list', $delta = 0) {
  global $user;
  switch ($op) {
    case 'list' :
      $blocks[0]['info'] = t('Current signups');
      return $blocks;
      break;
    case 'view' :
      if (user_access('access content')) {
        switch ($delta) {
          case 0:
            $titles = signup_list_user_signups($user->uid);
            if (count($titles)) {
              $block['subject'] = t('Current signups');
              $block['content'] = theme_item_list($titles) . l(t('View signup schedule'), "user/$user->uid/signups");
            }
            return $block;
        }
      }
  }
}

/**
 * Private helper as a partial implementation of hook_user().
 * @see signup_user()
 */
function _signup_user_no_views($op, &$edit, &$user, $category = NULL) {
  switch ($op) {
    case 'view':
      // grab list of events the user signed up for.
      $signups = signup_list_user_signups($user->uid);
      if (count($signups)) {
        $output = '<h4>'. t('Current signups') .' -- '. l(t('view signup schedule'), "user/$user->uid/signups") .'</h4>'. theme_item_list($signups);
      }
      if (isset($output)) {
        return array(t('Signup information') => array(array('value' => $output, 'class' => 'user')));
      }
  }
}

/**
 * Menu items we only need to define if views is not enabled.
 */
function signup_no_views_menu(&$items, $may_cache) {
  global $user;
  $access = user_access('administer all signups');
  if (!$may_cache) {
    // User signup schedule callback
    $items[] = array(
      'path' => 'user/'. arg(1) .'/signups',
      'access' => ($access || ($user->uid == arg(1))),
      'type' => MENU_CALLBACK,
      'callback' => 'signup_user_schedule',
      'callback arguments' => array($uid => arg(1)),
    );
  }
}

/**
 * Prints a schedule of the given user's signups.
 * @ingroup signup_callback
 */
function signup_user_schedule($uid) {
  $output = '';
  $user = user_load(array('uid' => $uid));
  if (!$user) {
    drupal_not_found();
    return;
  }
  include_once(SIGNUP_PATH .'/theme/signup.theme');
  drupal_set_title(t('Signups for @user', array('@user' => $user->name)));
  $titles = signup_list_user_signups($user->uid);
  foreach ($titles as $nid => $title) {
    $node = node_load(array('nid' => $nid));
    $output .= theme('signup_user_schedule', $node);
  }
  return $output;
}
