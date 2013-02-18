Drupal.behaviors.lookAhead = {};

Drupal.behaviors.lookAhead.attach = function(context) {
    function look_ahead_load(link) {
        link = jQuery(link);

        if (link.hasClass('look-ahead-processed')) {
            return;
        }

        var url = link.attr('data-look-ahead');

        if (url) {
            jQuery.get(url, function (data) {
                jQuery(data).insertAfter(link);
                link.addClass('look-ahead-processed');
            });
        }
    }

    var lookAheadLinks = jQuery('a.look-ahead').not('.look-ahead-processed');

    if (jQuery().hoverIntent) {
        lookAheadLinks.hoverIntent(function () {
            look_ahead_load(this);
        }, function () {});
    } else {
        lookAheadLinks.hover(function() {
            look_ahead_load(this);
        });
    }
};