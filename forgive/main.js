function selectButton(selectedButtonId, otherButtonId) {
  const selectedButton = document.getElementById(selectedButtonId)
  const otherButton = document.getElementById(otherButtonId)

  selectedButton.classList.remove('otherButtonSelected');
  selectedButton.classList.add('buttonSelected');

  otherButton.classList.add('otherButtonSelected');
  otherButton.classList.remove('buttonSelected');
}
