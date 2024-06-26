const submit = document.getElementById("formButtonSubmin");

submit.addEventListener("click", validate);

function validate(e) {

  const email = document.getElementById("email");
  const emailConf = document.getElementById("emailConfirmation");

  if (email.value != emailConf.value) {
       e.preventDefault();
  }
}