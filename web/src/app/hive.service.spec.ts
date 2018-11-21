import { TestBed } from '@angular/core/testing';

import { HiveService } from './hive.service';

describe('HiveService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: HiveService = TestBed.get(HiveService);
    expect(service).toBeTruthy();
  });
});
