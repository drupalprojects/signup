* signup_ecommerce
* Originally written by: Chad Phillips (http://drupal.org/user/22079)

NOTE: this module requires both the ecommerce and signup modules to be
installed and properly configured.

signup_ecommerce provides basic integration between the ecommerce
module and the signup module, specifically allowing automatic user
signup upon purchase of an enabled product type.

To enable the feature, first enable the module at:
  administer >> modules
  (admin/modules)

Then, go to:
  administer >> settings >> content types
  (admin/settings/content-types)

Click 'configure' for the node type you wish to enable.  Make sure
that the 'Allow signups by default' and 'Signup Ecommerce Integration'
checkboxes are checked, and choose when you would like the autosignups
to occur with the 'Workflow' selection.  Note that the node type must
also be product enabled, and any product nodes must be properly
configured for the functionality to work.

For the node type you just enabled, any time somebody purchases a
product of that node type, they will automatically be signed up for
that node.  The signup information will include the transaction ID for
reference.

