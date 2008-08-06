<?php
// $Id$

/**
 * @return Array of SQL clauses for cron reminder email query builder.
 */
function signup_reminder_sql() {
  // We must manually include this here as the event module doesn't
  // include timezone support on all page requests.
  include_once(drupal_get_path('module', 'event') .'/event_timezones.inc');
  return array(
    'fields' => array('e.event_start', 'e.timezone'),
    'joins' => array('INNER JOIN {event} e ON e.nid = n.nid'),
    'where' => array('('. time() .' + ((s.reminder_days_before) * 86400)) > e.event_start'),
  );
}

/**
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function signup_autoclose_sql() {
  // We must manually include this here as the event module doesn't
  // include timezone support on all page requests.
  include_once(drupal_get_path('module', 'event') .'/event_timezones.inc');
  return array(
    'fields' => array('e.event_start', 'e.timezone'),
    'joins' => array('INNER JOIN {event} e ON e.nid = s.nid'),
    'where' => array('e.event_start < '. (time() + (variable_get('signup_close_early', 1) * 3600))),
  );
}

/**
 * @return Array of SQL clauses for admin overview page query builder.
 */
function signup_admin_sql() {
  $fields = array('e.event_start', 'e.timezone');
  return array(
    'fields' => $fields,
    // All of the fields should go directly into GROUP BY.
    'group_by' => $fields,
    'joins' => array('LEFT JOIN {event} e ON e.nid = n.nid'),
  );
}

function signup_admin_form_header() {
  return array('data' => t('Start'), 'field' => 'e.event_start');
}

function signup_admin_form_extra($signup_event) {
  return array(
    '#type' => 'markup',
    '#value' => signup_format_date($signup_event),
  );
}

function signup_list_user_signups_sql() {
  return "SELECT n.nid, n.title, e.event_start FROM {node} n INNER JOIN {signup_log} s_l ON n.nid = s_l.nid LEFT JOIN {event} e ON e.nid = n.nid WHERE s_l.uid = %d AND (e.event_start >= ". time() ." OR e.event_start IS NULL) ORDER BY e.event_start";
}

/**
 * Returns true if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_event_completed($node) {
  if (is_numeric($node)) {
    $node = node_load($node);
  }
  if (isset($node->event_start)) {
    $closing_time = time() + (variable_get('signup_close_early', 1) * 3600);
    if ($node->event_start < $closing_time) {
      return TRUE;
    }
  }
  return FALSE;
}

function signup_extra_tokens() {
  return array('%time');
}

function signup_format_date($node) {
  include_once(drupal_get_path('module', 'event') .'/event_timezones.inc');
  $offset = intval(variable_get('date_default_timezone', 0));
  return $node->event_start ? _event_date(variable_get('signup_date_string', 'D, M jS, g:i A'), $node->event_start, $offset) : t('[Untimed]');
}

/**
 * Theme function for displaying the start and end times for a given node on
 * the signup schedule page.
 */
function theme_signup_event_dates($node) {
  // Event-based data, only if it's an event node.
  $output = '';
  if (isset($node->event_start)) {
    $output .= '<div class="signup-user-schedule"><div class="'. $node->type .'signup-start"><label>'. t('Start: ') .'</label>'. $node->start_format .'</div></div>';
    if ($node->event_start != $node->event_end) {
      $output .= '<div class="signup-user-schedule"><div class="'. $node->type .'signup-end"><label>'. t('End: ') .'</label>'. $node->end_format .'</div></div>';
    }
    if (variable_get('configurable_timezones', 1)) {
      $zones = event_zonelist();
      $output .= '<div class="signup-user-schedule"><div class="'. $node->type .'signup-tz"><label>'. t('Timezone: ') .'</label>'. $zones[$node->timezone] .'</div></div>';
    }
  }
  else {
    $output .= '<div class="signup-user-schedule"><div class="'. $node->type .'signup-untimed">'. t('[Untimed]') .'</div></div>';
  }
  return $output;
}

/**
 * Does this site have any date-enabled content?
 *
 * @return Bool: Always return TRUE in this backend
 */
function signup_site_has_dates() {
  return TRUE;
}

/**
 * @return Bool: Is this node type date-enabled?
 */
function signup_node_type_has_date($type) {
  return variable_get('event_nodeapi_'. $type, 'never') != 'never';
}

/**
 * @return Bool: Is this specific node date-enabled?
 */
function signup_node_has_date($node) {
  return isset($node->event_start);
}
