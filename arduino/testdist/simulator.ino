void simulatorStepRun() {
  int elapsedRPM, elapsedMS, elapsedmicros;

  initOrUpdate(&lastRPMUpdate, &elapsedRPM, ms);
  initOrUpdate(&startMS, &elapsedMS, ms);
  initOrUpdate(&startmicros, &elapsedmicros, micros_);

  fakeIncrementInterruptIfTime();
  fakeIndexPulseInterruptIfTime();
  fakeDistributorInterruptIfTime();
    
  fakeIncrementInterruptIfTime();
  fakeIndexPulseInterruptIfTime();
  fakeDistributorInterruptIfTime();
  
  if (elapsedmicros > 600) {
    incrementAngleEdge(); // FAKE INTERRUPT FIRE
  }
  if (!indexFired && elapsedMS > 200/fakeSpeed) {
    zeroAnglePulse(); // FAKE INTERRUPT FIRE
    indexFired = true;
  }
  if (elapsedMS > 275/fakeSpeed) {
    distributorFired(); // FAKE INTERRUPT
    startMS = ms;
    indexFired = false;
    sendStatus();
  }
}

