<?php
// $Id$

/**
 * @return Array of SQL clauses for cron reminder email query builder.
 */
function _signup_event_reminder_sql($content_type) {
  global $db_type;
  event_include_files();
  switch ($db_type) {
    case 'mysql':
    case 'mysqli':
      $where = array("('". date('Y-m-d H:i:s') ."' + INTERVAL s.reminder_days_before DAY ) > ". event_where_utc());
      break;
    case 'pgsql':
      $where = array("('". date('Y-m-d H:i:s') ."' + INTERVAL 's.reminder_days_before days' ) > ". event_where_utc());
  }
  return array(
    'fields' => array(event_select(), 'e.timezone'),
    'joins' => array(event_join()),
    'where' => $where,
  );
}

/**
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function _signup_event_autoclose_sql($content_type) {
  event_include_files();
  return array(
    'fields' => array(event_select(), 'e.timezone'),
    'joins' => array(event_join('s')),
    'where' => array(event_where_utc() ." < '". date('Y-m-d H:i:s', time() + variable_get('signup_close_early', 1) * 3600) ."'"),
  );
}

/**
 * @return Array of SQL clauses for admin overview page query builder.
 */
function _signup_event_admin_sql($content_type = NULL) {
  return array(
    'fields' => array(event_select(), 'e.timezone'),
    'group_by' => array('event_start', 'e.timezone'),
    'joins' => array(event_join('n', 'LEFT')),
  );
}

/**
 * Returns true if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_event_node_completed($node) {
  if (isset($node->event)) {
    $closing_time = date('Y-m-d H:i:s', time() + (variable_get('signup_close_early', 1) * 3600));
    if (event_is_later($closing_time, $node->event['start_utc'], 'string')) {
      return TRUE;
    }
  }
  return FALSE;
}

function _signup_event_format_date($node) {
  $event_date = array();
  if (!empty($node->event['start'])) {
    $event_date = event_explode_date($node->event['start']);
  }
  elseif (!empty($node->event_start)) {
    $event_date = event_explode_date($node->event_start);
  }
  return !empty($event_date) ? event_format_date($event_date, variable_get('signup_date_format', 'medium')) : t('[Untimed]');
}

/**
 * Determine if the given node has a start time managed by the event.module.
 *
 * @param $node
 *   Fully loaded node object to test.
 *
 * @return
 *   'event' if this is an event node, otherwise 'none'.
 *
 * @see _signup_get_node_scheduler()
 */
function _signup_event_get_node_scheduler($node) {
  if (isset($node->event) && isset($node->event['start'])) {
    return 'event';
  }
  return isset($node->event_start) ? 'event' : 'none';
}
