<?php
// $Id$

/**
 * Filter nodes based on their signup status (open vs. closed).
 */
class signup_handler_filter_signup_status extends views_handler_filter_boolean_operator {
  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }
    return (empty($this->value) ? t('Closed') : t('Open'));
  }
}

