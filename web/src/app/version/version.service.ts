import { Injectable } from '@angular/core';
import { RestService } from '../rest.service';
import { Observable } from 'rxjs';
import { retry, catchError } from 'rxjs/operators';
import { Version } from './version';

@Injectable({
  providedIn: 'root'
})
export class VersionService {
  constructor(private rest: RestService) { }

  get(): Observable<Version> {
    return this.rest.get<Version>('version')
                    .pipe( retry(3), catchError(this.rest.handleError<Version>('VersionService.get')) );
  }
}
