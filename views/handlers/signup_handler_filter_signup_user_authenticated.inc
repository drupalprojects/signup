<?php
// $Id$

/**
 * Filter signups based on if the user is anonymous or authenticated.
 */
class signup_handler_filter_signup_user_authenticated extends views_handler_filter_boolean_operator {
  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    return (empty($this->value) ? t('Anonymous') : t('Authenticated'));
  }

  function query() {
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.$this->real_field ". ($this->value ? '!= ' : '= ') ."0");
  }
}

