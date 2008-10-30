<?php
// $Id$

/**
 * @file
 * Date-specific functions used by signup when none of the supported event or
 * date management modules are enabled on the site.
 */

/**
 * Return the SQL needed to generate a given users signup schedule.
 *
 * The call site assumes the one and only %d in this query is the user's uid.
 */
function signup_list_user_signups_sql($content_type = NULL) {
  return "SELECT n.nid, n.title FROM {node} n INNER JOIN {signup_log} s_l ON n.nid = s_l.nid WHERE s_l.uid = %d ORDER BY n.nid";
}

/**
 * @return FALSE to indicate that the given node is not event-enabled.
 */
function _signup_event_completed($node) {
  return FALSE;
}

function signup_format_date($node) {
  return t('[Untimed]');
}

/**
 * Does this site have any date-enabled content?
 *
 * @return Bool: Always return FALSE in this backend
 */
function signup_site_has_dates() {
  return FALSE;
}

/**
 * Is this node type date-enabled?
 *
 * @return Bool: Always return FALSE in this backend
 */
function signup_node_type_has_date($type) {
  return FALSE;
}

/**
 * Is this specific node date-enabled?
 *
 * @return Bool: Always return FALSE in this backend
 */
function signup_node_has_date($node) {
  return FALSE;
}
