import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class RestService {
  private headers = new HttpHeaders({
    'Content-Type':  'application/json'
  });

  constructor(private http: HttpClient) {
  }

  getEndpointUri(): string {
    return window.location.origin + '/api/v1/';
  }

  public get<T>(uri: string, search?: URLSearchParams): Observable<T> {
    const options = { headers: this.headers, search: search };
    return this.http.get<T>(this.getEndpointUri() + uri, options);
  }

  public post<T>(uri: string, data: Object): Observable<T> {
    const options = { headers: this.headers };
    return this.http.post<T>(this.getEndpointUri() + uri, JSON.stringify(data), options);
  }
}
