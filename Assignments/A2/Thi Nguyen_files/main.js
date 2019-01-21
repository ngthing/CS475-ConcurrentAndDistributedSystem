/**
 * Created by thinguyen on 10/7/16.
 */
$(document).ready(function(){
    // Initialize Tooltip
    $('[data-toggle="tooltip"]').tooltip();

    // Add smooth scrolling to all links in navbar + footer link
    $(".navbar a, footer a[href='#myPage']").on('click', function(event) {

        // Make sure this.hash has a value before overriding default behavior
        if (this.hash !== "") {

            // Prevent default anchor click behavior
            event.preventDefault();

            // Store hash
            var hash = this.hash;

            // Using jQuery's animate() method to add smooth page scroll
            // The optional number (900) specifies the number of milliseconds it takes to scroll to the specified area
            $('html, body').animate({
                scrollTop: $(hash).offset().top
            }, 900, function(){

                // Add hash (#) to URL when done scrolling (default click behavior)
                window.location.hash = hash;
            });
        } // End if
    });
})

// Initialize Firebase
var config = {
    apiKey: "AIzaSyDZV95i6W3TPzh_bzAYRjmGmXMznrQmkCE",
    authDomain: "tnguy138site.firebaseapp.com",
    databaseURL: "https://tnguy138site.firebaseio.com",
    storageBucket: "",
    messagingSenderId: "275177408175"
};
firebase.initializeApp(config);

$(function(){
    //make a variable to keep track of the data coming from Firebase

    //create a new connection to firebase
    var commentsRef = firebase.database().ref("comments");

    $('#newComment').submit(function(event){
        var $form = $(this);
        var sender = $('#name').val();
        var email = $('#email').val();
        var comments = $('#comments').val();
        console.log(Date());
        //send the new data to Firebase
        commentsRef.push({
            sender: sender,
            email: email,
            comments: comments,
            time: Date()
        });

        //Show successfull message as a pop-up model
        $('#commentConfirm').html(
            "<p>Thanks, " + sender + " for your note! <br/> " +
            "I will get back to you as soon as I can. Have a great day! &#9752 </p>")
        $('#commentConfirm').show();
        //Clear form:
        document.getElementById('newComment').reset();
        //return false to prevent page from refreshing
        return false;
    });
});