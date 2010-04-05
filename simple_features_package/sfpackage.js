$ = jQuery;
$(document).ready(function() {
  $('.package-layer').hide();

  $('.package-select li').mouseenter(function() {
    $(this).children().children('.package-layer').fadeIn('normal');  
  }).mouseout(function() {
    $(this).children().children('.package-layer').fadeOut('normal');
  });

});
