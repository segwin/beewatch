import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Observable, of } from 'rxjs';

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

  public get<T>(relativeUri: string, params?: HttpParams): Observable<T> {
    let uri = this.getEndpointUri() + relativeUri;

    if (params) {
      uri += '?' + params.toString();
    }

    return this.http.get<T>(uri, { headers: this.headers});
  }

  public put<T>(relativeUri: string, data: Object): Observable<T> {
    const uri = this.getEndpointUri() + relativeUri;
    return this.http.put<T>(uri, JSON.stringify(data), { headers: this.headers});
  }

  public delete<T>(relativeUri: string, params?: HttpParams): Observable<T> {
    const uri = this.getEndpointUri() + relativeUri + '?' + params.toString();
    return this.http.delete<T>(uri, { headers: this.headers});
  }

  public handleError<T>(operation = 'operation', result?: T) {
    return (error: any): Observable<T> => {
      console.log(`${operation} failed: ${error.message}`);

      // Let the app keep running by returning an empty result.
      return of(result as T);
    };
  }
}
