<?php
// $Id$

/**
 * Filter nodes based on if signups are disabled or enabled.
 */
class signup_handler_filter_signup_disabled extends views_handler_filter_boolean_operator {
  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    return (empty($this->value) ? t('Disabled') : t('Enabled'));
  }

  function query() {
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.nid " . (empty($this->value) ? 'IS ' : 'IS NOT ') . "NULL");
  }
}

