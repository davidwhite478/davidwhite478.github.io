function selectButton(selectedButtonId, otherButtonId) {
  const selectedButton = document.getElementById(selectedButtonId)
  const otherButton = document.getElementById(otherButtonId)

  if (selectedButton.classList.contains('otherButtonSelected')) {
    clearSelections(selectedButton, otherButton);
  } else {
    selectedButton.classList.remove('otherButtonSelected');
    selectedButton.classList.add('buttonSelected');

    otherButton.classList.add('otherButtonSelected');
    otherButton.classList.remove('buttonSelected');
  }
}


function clearSelections(selectedButton, otherButton) {
  selectedButton.classList.remove('buttonSelected');
  selectedButton.classList.remove('otherButtonSelected');
  otherButton.classList.remove('buttonSelected');
  otherButton.classList.remove('otherButtonSelected');
}
