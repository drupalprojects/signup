/* $Id$ */

/**
 * On a node type settings form, if the "Allow signups" radios are 
 * not set to 0 ('Disabled'), then show the date field selection,
 * otherwise, hide it.
 */
Drupal.signupShowDateFieldAutoAttach = function () {
  $('div.signup-node-default-state-radios input[@type=radio]').click(function () {
    if (this.value == 'disabled') {
      $('div.signup-date-field-setting').hide();
    }
    else {
      $('div.signup-date-field-setting').show();
    }
  });
};

// Global killswitch.
if (Drupal.jsEnabled) {
  $(function() {
    Drupal.signupShowDateFieldAutoAttach();
  });
}
