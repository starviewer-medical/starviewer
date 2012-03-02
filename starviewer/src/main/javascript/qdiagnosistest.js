
$('document').ready(function()
{
    $('.errorTest, .warningTest, .succeededTest').addClass('closed');
    $('#errorButton, #warningButton').addClass('pressed');
    $('.succeededTest').hide();

    $('.errorTest .result, .warningTest .result').click(function()
    {
        var $test = $(this).parent();
        if ($test.hasClass('closed'))
        {
            $test.removeClass('closed');
            $test.siblings().addClass('closed');
        }
        else
        {
            $test.addClass('closed');
        }
    });

    $('#succeededButton').click(function()
    {
        buttonClicked($(this),'.succeededTest');
    });
    $('#errorButton').click(function()
    {
        buttonClicked($(this), '.errorTest');
    });
    $('#warningButton').click(function()
    {
        buttonClicked($(this), '.warningTest');
    });
});

function buttonClicked($button, itemsToEdit)
{
    var isPressed = !$button.hasClass('pressed');
    if (isPressed)
    {
        $button.addClass('pressed');
        $(itemsToEdit).show();
    }
    else
    {
        $button.removeClass('pressed');
        $(itemsToEdit).addClass('closed');
        $(itemsToEdit).hide();
    }
};

function showAllTests()
{
    $('.errorTest, .warningTest, .succeededTest').show();
    $('#succeededButton, #errorButton, #warningButton').addClass('pressed');
}
