/* $Id$ */

/**
 * On a node form, if the "Allow signups" radios are set to 1
 * ('Enabled'), then show the other signup-specific settings,
 * otherwise, hide them.
 */
Drupal.signupShowSettingsAutoAttach = function () {
  $('div.signup-allow-radios input[@type=radio]').click(function () {
    $('div.signup-node-settings')[['hide', 'show', 'hide'][this.value]]();
  });
}

// Global killswitch.
if (Drupal.jsEnabled) {
  $(function() {
    Drupal.signupShowSettingsAutoAttach();
  });
}
