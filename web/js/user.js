document.addEventListener("DOMContentLoaded", function () {
  // call functions on refresh
  loadSimulatedData();
  setupBitmaskDialog();
});

// open bitmask help dialog
function openGrpMaskHelp(button) {
  const dialog = document.getElementById("p12_bitmask_dialog");
  if (dialog) {
    dialog.showModal();
  } else {
    console.error("Dialog mit ID 'p12_bitmask_dialog' wurde nicht gefunden.");
  }
}

// close bitmask help dialog
function closeGrpMaskHelp() {
  const dialog = document.getElementById("p12_bitmask_dialog");
  if (dialog) {
    dialog.close();
  } else {
    console.error("Dialog mit ID 'p12_bitmask_dialog' wurde nicht gefunden.");
  }
}

function validateJaroliftSerial(input) {
  validateHex(input);

  const valid = /^0[0-9a-f]{5}$/.test(input.value);
  input.setAttribute("aria-invalid", valid ? "false" : "true");
}

function setupBitmaskDialog() {
  const bitmaskDialog = document.getElementById("bitmask_dialog");
  const applyButton = document.getElementById("apply_bitmask");
  const closeButton = document.getElementById("close_bitmask_dialog");

  let currentInput = null; // reference to existing input

  // open the dialog for the source input field
  document.querySelectorAll(".bitmask-input").forEach((input) => {
    input.addEventListener("click", () => {
      currentInput = input;

      // Parse the bitmask as a mutable variable
      let bitmask = parseInt(input.value || "0", 2);

      // Set checkboxes based on actual bitmask
      for (let i = 0; i < 16; i++) {
        const checkbox = document.getElementById(`channel-${i}`);
        if (checkbox) {
          // Check the visibility of the checkbox
          const isVisible = checkbox.parentElement.style.display !== "none";

          // Set checkbox state only if visible
          checkbox.checked = isVisible && (bitmask & (1 << i)) !== 0;

          // Clear bit if the checkbox is not visible
          if (!isVisible) {
            bitmask &= ~(1 << i);
          }
        }
      }

      // Safely update the input value with the sanitized bitmask
      if (currentInput && !Object.isFrozen(currentInput)) {
        currentInput.value = bitmask.toString(2).padStart(16, "0");
      }

      bitmaskDialog.showModal();
    });
  });

  // apply checkboxes and close the dialog
  applyButton.addEventListener("click", () => {
    let bitmask = 0;
    for (let i = 0; i < 16; i++) {
      const checkbox = document.getElementById(`channel-${i}`);
      if (checkbox && checkbox.checked) {
        bitmask |= 1 << i;
      }
    }

    if (currentInput) {
      currentInput.value = bitmask.toString(2).padStart(16, "0");
    }

    bitmaskDialog.close();
  });

  // close the dialog without changes
  closeButton.addEventListener("click", () => {
    bitmaskDialog.close();
  });
}

async function loadSimulatedData() {
  if (!isGitHubPages()) {
    return; // Kein Simulationsmodus, wenn nicht auf GitHub Pages
  }

  console.log("GitHub Pages erkannt – Simulationsdaten werden geladen.");

  try {
    const response = await fetch("sim.json");
    if (!response.ok)
      throw new Error("Fehler beim Abrufen der Simulationsdaten");

    const simData = await response.json();
    updateJSON(simData); // Aktualisiert die UI mit den Simulationsdaten
  } catch (error) {
    console.error("Fehler beim Laden von sim.json:", error);
  }
}

function updateUIcallbackSelect(elementId, value) {
  const match = elementId.match(/^cfg_timer_(\d+)_type$/);
  if (match) {
    // The flags and comparison values are loaded after type in config.json.
    setTimeout(() => loadScheduleMode(match[1]), 0);
  }
}

function loadScheduleMode(timerId) {
  const mode = document.getElementById(`cfg_timer_${timerId}_mode`);
  const type = document.getElementById(`cfg_timer_${timerId}_type`);
  const useMinTime = document.getElementById(
    `cfg_timer_${timerId}_use_min_time`
  );
  const useMaxTime = document.getElementById(
    `cfg_timer_${timerId}_use_max_time`
  );

  const legacyMode =
    type.value !== "0" && useMinTime.checked && useMaxTime.checked;

  if (type.value === "0") {
    mode.value = "0";
  } else if (legacyMode) {
    mode.value = "";
  } else if (useMinTime.checked) {
    mode.value = "2";
  } else if (useMaxTime.checked) {
    mode.value = "3";
  } else {
    mode.value = "1";
  }

  updateScheduleInputVisibility(timerId, legacyMode);
}

function toggleScheduleInputs(selectElement) {
  const timerId = selectElement.id.match(/^cfg_timer_(\d+)_mode$/)[1];
  const type = document.getElementById(`cfg_timer_${timerId}_type`);
  const useMinTime = document.getElementById(
    `cfg_timer_${timerId}_use_min_time`
  );
  const useMaxTime = document.getElementById(
    `cfg_timer_${timerId}_use_max_time`
  );

  if (selectElement.value !== "0" && type.value !== "1" && type.value !== "2") {
    type.value = "1";
  }

  useMinTime.checked = selectElement.value === "2";
  useMaxTime.checked = selectElement.value === "3";
  updateScheduleInputVisibility(timerId, false);
}

function updateScheduleInputVisibility(timerId, legacyMode) {
  const mode = document.getElementById(`cfg_timer_${timerId}_mode`).value;
  const timeInput = document.getElementById(`timeInput${timerId}`);
  const astroInput = document.getElementById(`astroInput${timerId}`);
  const offsetInput = document.getElementById(`offsetInput${timerId}`);
  const comparisonSettings = document.getElementById(
    `timer${timerId}-minmaxtime-settings`
  );
  const minTimeInput = document.getElementById(`minTimeInput${timerId}`);
  const maxTimeInput = document.getElementById(`maxTimeInput${timerId}`);
  const legacyWarning = document.getElementById(`legacyModeWarning${timerId}`);

  const showAstro = mode === "1" || mode === "2" || mode === "3" || legacyMode;
  const showComparison = mode === "2" || mode === "3";

  timeInput.style.display = mode === "0" ? "block" : "none";
  astroInput.style.display = showAstro ? "block" : "none";
  offsetInput.style.display = showAstro ? "block" : "none";
  comparisonSettings.style.display = showComparison ? "block" : "none";
  minTimeInput.style.display = mode === "2" ? "block" : "none";
  maxTimeInput.style.display = mode === "3" ? "block" : "none";
  legacyWarning.style.display = legacyMode ? "block" : "none";
}
