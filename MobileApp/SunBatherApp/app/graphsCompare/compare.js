import { CompareViewModel } from './compare-view-model'

export function onNavigatingTo(args) {
  const component = args.object;
  component.bindingContext = new CompareViewModel();
}